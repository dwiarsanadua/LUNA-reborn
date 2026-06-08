--[[
  Quest 371: [Party] Final Vengeance
  Level Required: 40
  NPC Start: 19, NPC Complete: 19
  Prerequisites: [370]
  Rewards: EXP=95297
]]

local fsm = require('fsm_engine')

local quest_371 = fsm:new({
    id = 371,
    name = "[Party] Final Vengeance",
    level_required = 40,
    npc_start = 19,
    npc_complete = 19,
    prerequisites = {370},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 371: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 371: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 371: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 161,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 371: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 371: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 371: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 371: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 371: Rewards given")
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

return quest_371