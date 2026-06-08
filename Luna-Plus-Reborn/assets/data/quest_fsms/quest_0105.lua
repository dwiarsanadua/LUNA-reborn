--[[
  Quest 105: Securing The Trade Route
  Level Required: 16
  NPC Start: 55, NPC Complete: 55
  Prerequisites: [152]
  Rewards: EXP=3512, Gold=2505
]]

local fsm = require('fsm_engine')

local quest_105 = fsm:new({
    id = 105,
    name = "Securing The Trade Route",
    level_required = 16,
    npc_start = 55,
    npc_complete = 55,
    prerequisites = {152},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 105: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 105: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 105: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 48,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 105: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 49,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 105: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 42,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 105: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 105: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 105: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 105: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 105: Rewards given")
                self:give_rewards({
                    exp = 3512,
                    gold = 2505,
                    items = {
                        { item_id = 21000012, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_105