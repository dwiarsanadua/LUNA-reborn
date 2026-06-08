--[[
  Quest 493: [Special][Quest] Graffler
  Level Required: 57
  NPC Start: 117, NPC Complete: 117
  Rewards: EXP=810689, Gold=187340
]]

local fsm = require('fsm_engine')

local quest_493 = fsm:new({
    id = 493,
    name = "[Special][Quest] Graffler",
    level_required = 57,
    npc_start = 117,
    npc_complete = 117,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 493: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 493: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 493: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 395,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 493: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 493: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 493: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 493: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 493: Rewards given")
                self:give_rewards({
                    exp = 810689,
                    gold = 187340,
                    items = {
                        { item_id = 334, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_493