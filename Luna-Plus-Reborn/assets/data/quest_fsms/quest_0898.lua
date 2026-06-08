--[[
  Quest 898: Bluebeard story
  Level Required: 99
  NPC Start: 536, NPC Complete: 537
  Rewards: EXP=9641761, Gold=77721
]]

local fsm = require('fsm_engine')

local quest_898 = fsm:new({
    id = 898,
    name = "Bluebeard story",
    level_required = 99,
    npc_start = 536,
    npc_complete = 537,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 898: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 536,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 898: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 898: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 294,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 898: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 536,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 898: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 898: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 898: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 898: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 898: Rewards given")
                self:give_rewards({
                    exp = 9641761,
                    gold = 77721,
                })
            end,
            transitions = {},
        },
    },
})

return quest_898