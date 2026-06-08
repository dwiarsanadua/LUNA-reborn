--[[
  Quest 94: Credit Recovery
  Level Required: 14
  NPC Start: 16, NPC Complete: 16
  Rewards: EXP=3236, Gold=1560
]]

local fsm = require('fsm_engine')

local quest_94 = fsm:new({
    id = 94,
    name = "Credit Recovery",
    level_required = 14,
    npc_start = 16,
    npc_complete = 16,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 94: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 16,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 94: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 94: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 68,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 94: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 16,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 94: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 94: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 16,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 94: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 94: Rewards given")
                self:give_rewards({
                    exp = 3236,
                    gold = 1560,
                    items = {
                        { item_id = 37, count = 1 },
                        { item_id = 21000008, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_94