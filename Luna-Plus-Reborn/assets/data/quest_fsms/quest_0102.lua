--[[
  Quest 102: Herbal Supplements
  Level Required: 15
  NPC Start: 17, NPC Complete: 20
  Rewards: EXP=1761, Gold=750
]]

local fsm = require('fsm_engine')

local quest_102 = fsm:new({
    id = 102,
    name = "Herbal Supplements",
    level_required = 15,
    npc_start = 17,
    npc_complete = 20,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 102: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 102: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 102: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 102: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 17,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 102: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 102: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 102: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 102: Rewards given")
                self:give_rewards({
                    exp = 1761,
                    gold = 750,
                    items = {
                        { item_id = 44, count = 1 },
                        { item_id = 44, count = 1 },
                        { item_id = 21000008, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_102