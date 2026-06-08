--[[
  Quest 34: Harpy Feathers
  Level Required: 41
  NPC Start: 37, NPC Complete: 37
  Prerequisites: [27]
  Rewards: EXP=110196, Gold=5820
]]

local fsm = require('fsm_engine')

local quest_34 = fsm:new({
    id = 34,
    name = "Harpy Feathers",
    level_required = 41,
    npc_start = 37,
    npc_complete = 37,
    prerequisites = {27},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 34: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 37,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 34: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 34: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 30,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 34: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 37,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 34: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 34: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 37,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 34: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 34: Rewards given")
                self:give_rewards({
                    exp = 110196,
                    gold = 5820,
                    items = {
                        { item_id = 13, count = 1 },
                        { item_id = 13, count = 10 },
                        { item_id = 30000035, count = 4 },
                        { item_id = 21000095, count = 8 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_34