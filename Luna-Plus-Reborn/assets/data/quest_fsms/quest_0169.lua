--[[
  Quest 169: Protect the Habitat
  Level Required: 36
  NPC Start: 30, NPC Complete: 30
  Prerequisites: [29]
  Rewards: EXP=78779, Gold=6540
]]

local fsm = require('fsm_engine')

local quest_169 = fsm:new({
    id = 169,
    name = "Protect the Habitat",
    level_required = 36,
    npc_start = 30,
    npc_complete = 30,
    prerequisites = {29},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 169: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 30,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 169: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 169: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 57,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 169: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 58,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 169: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 169: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 169: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 30,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 169: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 169: Rewards given")
                self:give_rewards({
                    exp = 78779,
                    gold = 6540,
                    items = {
                        { item_id = 30000172, count = 2 },
                        { item_id = 21000013, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_169