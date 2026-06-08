--[[
  Quest 70: Good Neighbor
  Level Required: 4
  NPC Start: 10, NPC Complete: 11
  Prerequisites: [69]
  Rewards: EXP=50
]]

local fsm = require('fsm_engine')

local quest_70 = fsm:new({
    id = 70,
    name = "Good Neighbor",
    level_required = 4,
    npc_start = 10,
    npc_complete = 11,
    prerequisites = {69},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 70: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 70: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 70: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 70: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 70: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 70: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 70: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 70: Rewards given")
                self:give_rewards({
                    exp = 50,
                    items = {
                        { item_id = 21000006, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_70