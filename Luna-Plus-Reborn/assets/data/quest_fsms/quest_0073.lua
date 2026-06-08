--[[
  Quest 73: Turtle Power
  Level Required: 7
  NPC Start: 1, NPC Complete: 1
  Rewards: EXP=420, Gold=1110
]]

local fsm = require('fsm_engine')

local quest_73 = fsm:new({
    id = 73,
    name = "Turtle Power",
    level_required = 7,
    npc_start = 1,
    npc_complete = 1,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 73: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 1,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 73: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 73: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 27,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 73: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 73: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 73: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 1,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 73: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 73: Rewards given")
                self:give_rewards({
                    exp = 420,
                    gold = 1110,
                    items = {
                        { item_id = 23, count = 1 },
                        { item_id = 23, count = 10 },
                        { item_id = 21001505, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_73