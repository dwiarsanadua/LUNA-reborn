--[[
  Quest 80: [Repeatable] Zank's First Request
  Level Required: 10
  NPC Start: 1, NPC Complete: 51
  Rewards: EXP=2464, Gold=1170
]]

local fsm = require('fsm_engine')

local quest_80 = fsm:new({
    id = 80,
    name = "[Repeatable] Zank's First Request",
    level_required = 10,
    npc_start = 1,
    npc_complete = 51,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 80: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 51,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 80: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 80: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 22,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 80: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 51,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 80: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 80: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 80: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 51,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 80: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 80: Rewards given")
                self:give_rewards({
                    exp = 2464,
                    gold = 1170,
                    items = {
                        { item_id = 29, count = 15 },
                        { item_id = 21000513, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_80