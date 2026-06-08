--[[
  Quest 370: [Party] Fiery Revenge
  Level Required: 40
  NPC Start: 19, NPC Complete: 19
  Rewards: EXP=95297
]]

local fsm = require('fsm_engine')

local quest_370 = fsm:new({
    id = 370,
    name = "[Party] Fiery Revenge",
    level_required = 40,
    npc_start = 19,
    npc_complete = 19,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 370: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 370: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 370: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 160,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 370: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 370: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 370: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 370: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 370: Rewards given")
                self:give_rewards({
                    exp = 95297,
                    items = {
                        { item_id = 30000121, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_370