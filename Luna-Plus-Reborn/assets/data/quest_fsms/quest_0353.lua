--[[
  Quest 353: [Party] Ecosystem Investigation
  Level Required: 35
  NPC Start: 59, NPC Complete: 59
  Prerequisites: [352]
  Rewards: EXP=93020, Gold=19000
]]

local fsm = require('fsm_engine')

local quest_353 = fsm:new({
    id = 353,
    name = "[Party] Ecosystem Investigation",
    level_required = 35,
    npc_start = 59,
    npc_complete = 59,
    prerequisites = {352},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 353: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 59,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 353: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 353: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 155,
                    count = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 353: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 157,
                    count = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 353: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 156,
                    count = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 353: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 154,
                    count = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 353: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 158,
                    count = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 353: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 59,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 353: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 353: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 59,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 353: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 353: Rewards given")
                self:give_rewards({
                    exp = 93020,
                    gold = 19000,
                    items = {
                        { item_id = 261, count = 20 },
                        { item_id = 262, count = 10 },
                        { item_id = 263, count = 5 },
                        { item_id = 264, count = 30 },
                        { item_id = 265, count = 10 },
                        { item_id = 21000095, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_353