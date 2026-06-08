--[[
  Quest 109: Handkerchief of Reconciliation
  Level Required: 16
  NPC Start: 58, NPC Complete: 58
  Prerequisites: [105]
  Rewards: EXP=4683, Gold=2320
]]

local fsm = require('fsm_engine')

local quest_109 = fsm:new({
    id = 109,
    name = "Handkerchief of Reconciliation",
    level_required = 16,
    npc_start = 58,
    npc_complete = 58,
    prerequisites = {105},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 109: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 58,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 109: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 109: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 6,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 109: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 48,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 109: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 49,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 109: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 58,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 109: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 109: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 58,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 109: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 109: Rewards given")
                self:give_rewards({
                    exp = 4683,
                    gold = 2320,
                    items = {
                        { item_id = 49, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_109