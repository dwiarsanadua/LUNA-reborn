--[[
  Quest 904: A Village Still in Ruins
  Level Required: 80
  NPC Start: 97, NPC Complete: 316
  Prerequisites: [903]
  Rewards: EXP=1114618
]]

local fsm = require('fsm_engine')

local quest_904 = fsm:new({
    id = 904,
    name = "A Village Still in Ruins",
    level_required = 80,
    npc_start = 97,
    npc_complete = 316,
    prerequisites = {903},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 904: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 904: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 904: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 904: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 316,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 904: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 904: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 316,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 904: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 904: Rewards given")
                self:give_rewards({
                    exp = 1114618,
                    items = {
                        { item_id = 21000010, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_904