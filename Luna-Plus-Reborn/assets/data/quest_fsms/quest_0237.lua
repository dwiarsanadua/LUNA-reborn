--[[
  Quest 237: [Repeatable] Threatdown: Red Bears
  Level Required: 26
  NPC Start: 29, NPC Complete: 29
  Rewards: EXP=78522, Gold=3000
]]

local fsm = require('fsm_engine')

local quest_237 = fsm:new({
    id = 237,
    name = "[Repeatable] Threatdown: Red Bears",
    level_required = 26,
    npc_start = 29,
    npc_complete = 29,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 237: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 237: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 237: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 50,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 237: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 237: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 237: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 237: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 237: Rewards given")
                self:give_rewards({
                    exp = 78522,
                    gold = 3000,
                    items = {
                        { item_id = 21000012, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_237