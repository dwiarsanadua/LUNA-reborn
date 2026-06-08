--[[
  Quest 881: Tackle it again!
  Level Required: 94
  NPC Start: 129, NPC Complete: 129
  Rewards: EXP=6450627, Gold=61188
]]

local fsm = require('fsm_engine')

local quest_881 = fsm:new({
    id = 881,
    name = "Tackle it again!",
    level_required = 94,
    npc_start = 129,
    npc_complete = 129,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 881: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 129,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 881: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 881: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 290,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 881: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 129,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 881: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 881: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 129,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 881: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 881: Rewards given")
                self:give_rewards({
                    exp = 6450627,
                    gold = 61188,
                    items = {
                        { item_id = 21000010, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_881