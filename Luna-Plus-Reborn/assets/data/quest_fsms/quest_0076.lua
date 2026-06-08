--[[
  Quest 76: [Repeatable] A Study of Spores
  Level Required: 6
  NPC Start: 6, NPC Complete: 6
  Rewards: EXP=298, Gold=1050
]]

local fsm = require('fsm_engine')

local quest_76 = fsm:new({
    id = 76,
    name = "[Repeatable] A Study of Spores",
    level_required = 6,
    npc_start = 6,
    npc_complete = 6,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 76: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 76: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 76: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 1,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 76: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 76: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 76: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 76: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 76: Rewards given")
                self:give_rewards({
                    exp = 298,
                    gold = 1050,
                    items = {
                        { item_id = 25, count = 1 },
                        { item_id = 25, count = 10 },
                        { item_id = 21000011, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_76